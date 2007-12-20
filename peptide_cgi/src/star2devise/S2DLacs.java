// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005-2007
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

public class S2DLacs {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _longName;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private String _title;

    public String _xCoordName;
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


    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DLacs(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DLacs.S2DLacs(" + name + ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;

	_line1 = new Line();
	_line2 = new Line();
	_lineIntersect = new XYPair();
	_lineIntersectValid = false;
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
	    _title = "LACS Data (" + _yCoordName + " vs. " +
	      _xCoordName + ")";

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
	      _name, frameIndex, info, _title);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), S2DUtils.TYPE_LACS,
	      _name, frameIndex, _title);
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
