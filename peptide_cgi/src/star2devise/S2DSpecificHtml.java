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

// This class is used to convert a "base" specific html files to an html file
// specific to a particular data set.

// ------------------------------------------------------------------------

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

    private static final String searchString1 = "ENTRY_IDd1.ds";
    private static final String searchString1b = "ENTRY_IDd1.html";
    private static final String searchString1c = "ENTRY_IDd1.jnlp";
    private static final String searchString2 = "bmrENTRY_ID.str";
    private static final String searchString3 = "HELP_FILE.html";
    private static final String searchString4 = "DUMMY_TITLE";
    private static final String searchString5 = "DUMMY_DETAILS";
    private static final String searchString6 = "ENTRY_IDy";
    private static final String searchString7 =
        "\"cmdport\" value=\"6666\"";
    private static final String searchString8 = "CGI_URL";
    private static final String searchString9 = "EXAMPLE_ID";

    // Allow substiting the plain entry ID
    private static final String searchString10 = "ENTRY_ID";

    private String _replaceString1;
    private String _replaceString1b;
    private String _replaceString1c;
    private String _replaceString2;
    private String _replaceString3;
    private String _replaceString4;
    private String _replaceString5;
    private String _replaceString6;
    private String _replaceString7;
    private String _replaceString8;
    private String _replaceString9;
    private String _replaceString10;

    private boolean _multiEntry = false;
    private boolean _isJmol = false;

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
                               fullName + ", " + frameIndex1 + ", " + frameIndex2 + ")");
        }

        _multiEntry = true;

        init(htmlDir, dataType, name, fullName, frameIndex1,
             frameIndex2, title, frameDetails);
    }

    //-------------------------------------------------------------------
    // Constructor (for s2predict).
    public S2DSpecificHtml(String htmlDir, int dataType, String name,
                           int coordIndex, int frameIndex, String title,
                           String frameDetails) throws S2DError
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DSpecificHtml.S2DSpecificHtml(" +
                               htmlDir + ", " + dataType + ", " + name + ", " +
                               coordIndex + ", " + frameIndex + ")");
        }

        init(htmlDir, dataType, name, name, coordIndex,
             frameIndex, title, frameDetails);
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
        String templateFile = TemplateFileName("specific_html_jmol.base");
        // Choose the template based on UVD or not
        String templateJNLPFile = _isUvd ? TemplateFileName("jnlp_uvd.base") : TemplateFileName("jnlp.base");
        writeOne(templateFile, templateJNLPFile);
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Get the template file name.
    protected String TemplateFileName(String template_name)
    {
        _isJmol = (_dataType == S2DUtils.TYPE_ATOMIC_COORDS) ||
                  (_dataType == S2DUtils.TYPE_TORSION_ANGLE) ||
                  (_dataType == S2DUtils.TYPE_RRTORSION_ANGLE) ||
                  (_dataType == S2DUtils.TYPE_DIST_RESTR) ||
                  (_dataType == S2DUtils.TYPE_RRDIST_RESTR);

        String templateFile = "html_templates" + File.separator;
        templateFile += template_name;

        return templateFile;
    }

    protected String OutFileName(String suffix) throws S2DError
    {
        String outFileName = _htmlDir + File.separator + _name +
                             File.separator + _fullName + _dataSuffix + _frameIndexStr +
                             suffix;
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
        line = S2DUtils.replace(line, searchString1c,
                                _replaceString1c);
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
        line = S2DUtils.replace(line, searchString8,
                                _replaceString8);
        line = S2DUtils.replace(line, searchString9,
                                _replaceString9);
        line = S2DUtils.replace(line, searchString10,
                                _replaceString10);
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
            if (dataType != S2DUtils.TYPE_S2PRED) {
                _frameIndexStr += "+" + frameIndex2;
            } else {
                _frameIndexStr += "-" + frameIndex2;
            }
        }
        _title = title;

        _dataSuffix = S2DUtils.dataType2Suffix(_dataType);
        _replaceString1 = _name + File.separator + _fullName + _dataSuffix +
                          _frameIndexStr + S2DNames.SESSION_SUFFIX;
        _replaceString1b =  _fullName + _dataSuffix +
                            _frameIndexStr + S2DNames.HTML_SUFFIX;
        _replaceString1c =  _fullName + _dataSuffix +
                            _frameIndexStr + S2DNames.JNLP_SUFFIX;
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
            _replaceString5 = "Frame details: " + frameDetails;
        } else {
            _replaceString5 = "";
        }

        _replaceString6 = name + S2DNames.SUMMARY_HTML_SUFFIX;
        _replaceString7 = "\"cmdport\" value=\"" +
                          S2DNames.JS_CMD_PORT + "\"";
        _replaceString8 = S2DNames.CGI_URL;

        _replaceString9 = _isUvd ? S2DMain.DEFAULT_ACCESSION_NUM : name;

        _replaceString10 = name;
    }

    //-------------------------------------------------------------------
    private void writeOne(String templateFile, String templateJNLPFile)
    throws S2DException
    {
        try {
            FileWriter writer = S2DFileWriter.create(
                                    OutFileName(S2DNames.HTML_SUFFIX));
            BufferedReader reader = new BufferedReader(
                new FileReader(templateFile));

            String inLine;
            while ((inLine = reader.readLine()) != null) {

                // Filter out Jmol line if necessary.
                if (!_isJmol) {
                    if (inLine.indexOf("3D rendering by") >= 0) {
                        continue;
                    }
                }

                String outLine = FilterLine(inLine);

                outLine = S2DUtils.replace(outLine, "COMMENT_EMAIL",
                                           S2DNames.COMMENT_EMAIL);

                if (_isUvd) {
                    outLine = S2DUtils.replace(outLine, "../..", "../../..");
                    outLine = S2DUtils.replace(outLine, "../js_videos.html",
                                               "../../js_videos.html");
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


        // Write out the JNLP file in the same style
        try {
            FileWriter writer = S2DFileWriter.create(
                                    OutFileName(S2DNames.JNLP_SUFFIX));
            BufferedReader reader = new BufferedReader(
                new FileReader(templateJNLPFile));

            String inLine;
            while ((inLine = reader.readLine()) != null) {
                String outLine = FilterLine(inLine);
                outLine = S2DUtils.replace(outLine, "DEVISE_HOST", S2DNames.HOST);
                outLine = S2DUtils.replace(outLine, "CODEBASE", S2DNames.CODEBASE);
                writer.write(outLine + "\n");
            }

            reader.close();
            writer.close();
        } catch(IOException ex) {
            System.err.println("IOException: " +
                               ex.toString() + " writing specific JNLP file");
            throw new S2DError("Can't write specific JNLP file");
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
