// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005-2013
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of LACS data.  For each set
// of LACS data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.9.14.1  2013/10/22 19:45:08  wenger
// Merged peak_lists_br_0 thru peak_lists_br_2 to peak_lists2_br.
//
// Revision 1.9.12.1  2013/06/06 19:31:42  wenger
// Changed the mode to make more use of the S2DUtils.string2Double()
// and S2DUtils.string2Int() methods.
//
// Revision 1.9  2011/10/10 23:43:39  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.8.14.1  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.8  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.7  2010/03/10 22:36:16  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.6  2008/06/04 21:12:01  wenger
// New Peptide-CGI summary page is implemented, test work except for
// test52 for some weird reason.  (Still may need some other changes
// before release, though.)
//
// Revision 1.5  2008/04/09 19:35:41  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.4  2007/12/20 16:49:03  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//
// Revision 1.3  2007/08/20 20:26:08  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.1.2.5  2005/11/02 23:21:29  wenger
// Changed LACS-related STAR file tags to be properly defined;
// horizontal line in LACS visualizations is at *minus* y offset.
//
// Revision 1.1.2.4  2005/11/02 21:13:24  wenger
// Changed ovals to text Os in LACs visualization so they stay the
// same size in *pixels*; fixed up titles for LACS stuff in summary
// html page, specific html pages, and sessions.
//
// Revision 1.1.2.3  2005/11/02 20:33:16  wenger
// LACS visualization now has correct axis labels.
//
// Revision 1.1.2.2  2005/11/02 17:00:37  wenger
// Changed LACS visualization to have horizontal and vertical reference
// lines, as requested by Hamid and Eldon; added visualization info view.
//
// Revision 1.1.2.1  2005/10/14 21:24:57  wenger
// Oops -- forgot to cvs add this before -- should have been part of
// initial LACS commit.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import EDU.bmrb.starlibj.SaveFrameNode;

public class S2DLacs {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _longName;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private String _frameDetails;

    private String _title;

    private String _xCoordName;
    public String _yCoordName;

    class XYPair {
        public double _x;
        public double _y;
    }

    class Line {
        public XYPair _point1;
        public XYPair _point2;
        public XYPair _center;
        public XYPair _size;

	public Line()
	{
	    _point1 = new XYPair();
	    _point2 = new XYPair();
	    _center = new XYPair();
	    _size = new XYPair();
	}

	public void findCenterSize()
	{
	    _center._x = (_point1._x + _point2._x) / 2;
	    _center._y = (_point1._y + _point2._y) / 2;
	    _size._x = _point2._x - _point1._x;
	    _size._y = _point2._y - _point1._y;
	}

	public String toString()
	{
	    return _center._x + " " + _center._y + " " + _size._x + " " +
	      _size._y;
	}
    }

    public Line _line1;
    public Line _line2;
    public double _yOffset;

    public int[] _resSeqCodes;
    public String[] _resLabels;
    public double[] _xCoords;
    public double[] _yCoords;
    public int[] _desigs;

    private XYPair _lineIntersect;
    private boolean _lineIntersectValid;

    private String _starVersion;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DLacs(String name, String longName, S2DStarIfc star,
      SaveFrameNode frame, String dataDir, String sessionDir,
      S2DSummaryHtml summary) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DLacs.S2DLacs(" + name + ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_frameDetails = star.getFrameDetails(frame);
	_starVersion = star.version();

	_line1 = new Line();
	_line2 = new Line();
	_lineIntersect = new XYPair();
	_lineIntersectValid = false;

        //
        // Get the values we need from the Star file.
        //
	_xCoordName = star.getTagValue(frame, star.LACS_X_NAME);
	_yCoordName = star.getTagValue(frame, star.LACS_Y_NAME);

	//TEMP -- catch NumberFormatExceptions here and report details?
	_line1._point1._x = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_LINE1_X1));
	_line1._point1._y = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_LINE1_Y1));
	_line1._point2._x = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_LINE1_X2));
	_line1._point2._y = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_LINE1_Y2));

	_line2._point1._x = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_LINE2_X1));
	_line2._point1._y = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_LINE2_Y1));
	_line2._point2._x = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_LINE2_X2));
	_line2._point2._y = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_LINE2_Y2));

	_yOffset = S2DUtils.string2Double(star.getTagValue(frame,
	  star.LACS_Y_OFFSET));

	calculateLines();

        String[] resSeqCodesTmp = star.getFrameValues(frame,
          star.LACS_RES_NUM, star.LACS_RES_NUM);
	_resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.LACS_RES_NUM);
	resSeqCodesTmp = null;

        _resLabels = star.getFrameValues(frame,
          star.LACS_RES_NUM, star.LACS_RES_LABEL);

        String[] xCoordsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_X_VALUE);
	_xCoords = S2DUtils.arrayStr2Double(xCoordsTmp,
	  star.LACS_X_VALUE);
	xCoordsTmp = null;

        String[] yCoordsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_Y_VALUE);
	_yCoords = S2DUtils.arrayStr2Double(yCoordsTmp,
	  star.LACS_Y_VALUE);
	yCoordsTmp = null;

        String[] desigsTmp = star.getFrameValues(frame,
	  star.LACS_RES_NUM, star.LACS_DESIGNATOR);
	_desigs = S2DUtils.arrayStr2Int(desigsTmp, star.LACS_DESIGNATOR);
	desigsTmp = null;
    }

    //-------------------------------------------------------------------
    // Convert the "two points" form of the lines to the "center and
    // size" form that DEVise understands.
    public void calculateLines()
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DLacs.calculateLines()");
	}

	_line1.findCenterSize();
	_line2.findCenterSize();
    }

    //-------------------------------------------------------------------
    // Write the LACS values for this data.
    public void writeLACS(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DLacs.writeLACS()");
	}

	try {
	    _title = _yCoordName + " vs. " + _xCoordName;

	    //
	    // Write the LACS line values to the line data file.
	    //
            FileWriter lacsWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.LACS_LINE_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);

	    // Write header.
	    lacsWriter.write("# Data: LACS for " + _name + "\n");
	    lacsWriter.write("# Schema: bmrb-lacs_line\n");
	    lacsWriter.write("# Attributes: center_x; " +
	      "center_y; size_x; size_y; line_type\n");
	    lacsWriter.write("# (Line_type: 0 = line fitted to data; " +
	      "1 = reference lines)\n");
            lacsWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            lacsWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    lacsWriter.write("#\n");

	    lacsWriter.write(_line1 + " 0\n");
	    lacsWriter.write(_line2 + " 0\n");

	    // Write horizontal line at Y = -_yOffset;
	    double lineY = -_yOffset;
	    lacsWriter.write("0.0 " + lineY + " 100.0 0.0 1\n");
	    // Note: the lines below are to give us some margin if we really
	    // zoom in; we can get rid of this once we transition to
	    // DEVise 1.7.19.  (See bugs 902, 916.)
	    lacsWriter.write("0.0 " + lineY + " 1.0 0.0 1\n");
	    lacsWriter.write("0.0 " + lineY + " 0.1 0.0 1\n");
	    lacsWriter.write("0.0 " + lineY + " 0.01 0.0 1\n");

	    // Write vertical line at X = 0;
	    lacsWriter.write("0.0 0.0 0.0 100.0 1\n");
	    // Note: the lines below are to give us some margin if we really
	    // zoom in; we can get rid of this once we transition to
	    // DEVise 1.7.19.  (See bugs 902, 916.)
	    lacsWriter.write("0.0 0.0 0.0 1.0 1\n");
	    lacsWriter.write("0.0 0.0 0.0 0.1 1\n");
	    lacsWriter.write("0.0 0.0 0.0 0.01 1\n");

	    lacsWriter.close();

	    //
	    // Write the LACS point values to the point data file.
	    //
            lacsWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.LACS_POINT_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);

	    // Write header.
	    lacsWriter.write("# Data: LACS for " + _name + "\n");
	    lacsWriter.write("# Schema: bmrb-lacs_point\n");
	    lacsWriter.write("# Attributes: Comp_index_ID; " +
	      "Comp_ID; X_coord_val; Y_coord_val; Designator\n");
            lacsWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            lacsWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    lacsWriter.write("#\n");

	    // Write points.
	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        lacsWriter.write(_resSeqCodes[index] + " " +
		  _resLabels[index] + " " + _xCoords[index] + " " +
		  _yCoords[index] + " " + _desigs[index] + "\n");
	    }

	    lacsWriter.close();

	    //
	    // Write the LACS coordinate names to the coordinate data file.
	    //
            lacsWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.LACS_COORD_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);

	    // Write header.
	    lacsWriter.write("# Data: LACS for " + _name + "\n");
	    lacsWriter.write("# Schema: bmrb-lacs_coord\n");
	    lacsWriter.write("# Attributes: x_coord_name; y_coord_name\n");

	    // Write actual coordinate names.
	    lacsWriter.write(_xCoordName + " " + _yCoordName + "\n");

	    lacsWriter.close();

            //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_LACS,
	      _name, frameIndex, info, _title, true, _starVersion, "");

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), S2DUtils.TYPE_LACS,
	      _name, frameIndex, _title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeLACS(_title, frameIndex);

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing LACS values: " +
	      ex.toString());
	    throw new S2DError("Can't write LACS values");
	}
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
