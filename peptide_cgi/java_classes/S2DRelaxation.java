// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of relaxation data.  For each set of
// relaxation data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2001/02/09 16:57:13  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//
// Revision 1.1  2001/01/19 15:39:06  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//

// ========================================================================

import java.io.*;

public class S2DRelaxation {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accessionNum;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private int _dataType;
    private String _suffix;
    private String _name;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _atomNames;
    private String[] _relaxationValues;
    private String[] _relaxationErrors;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  (See S2DUtils for dataType.)
    public S2DRelaxation(int accessionNum, String dataDir, String sessionDir,
      S2DSummaryHtml summary, int dataType, String frequency,
      String[] resSeqCodes, String[] resLabels, String[] atomNames,
      String[] relaxationValues, String[] relaxationErrors) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DRelaxation.S2DRelaxation(" + accessionNum +
	      ")");
	}
        _accessionNum = accessionNum;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;

	_dataType = dataType;
        switch (dataType) {
	case S2DUtils.TYPE_T1_RELAX:
	    _suffix = S2DNames.T1_SUFFIX;
	    _name = "T1 Relaxation";
	    break;

	case S2DUtils.TYPE_T2_RELAX:
	    _suffix = S2DNames.T2_SUFFIX;
	    _name = "T2 Relaxation";
	    break;

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

	_name += " (" + frequency + " MHz)";

        _resSeqCodes = resSeqCodes;
        _resLabels = resLabels;
        _atomNames = atomNames;
        _relaxationValues = relaxationValues;
        _relaxationErrors = relaxationErrors;
    }

    //-------------------------------------------------------------------
    // Write the relaxation values for this data.
    public void writeRelaxation(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DRelaxation.writeRelaxation()");
	}

	try {
	    //
	    // Write the relaxation values to the data file.
	    //
            FileWriter relaxWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + _suffix + frameIndex + S2DNames.DAT_SUFFIX);

	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        relaxWriter.write(_resSeqCodes[index] + " " +
		  _resLabels[index] + " " + _atomNames[index] + " " +
		  _relaxationValues[index] + " " + _relaxationErrors[index] +
		  "\n");
	    }

	    relaxWriter.close();

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, _dataType,
	      _accessionNum, frameIndex, _name);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, _dataType,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeRelax(_suffix, _name, frameIndex,
	      _resSeqCodes.length);

        } catch(IOException ex) {
	    System.err.println("IOException writing relaxation data: " +
	      ex.getMessage());
	    throw new S2DError("Can't write relaxation data");
	}
    }
}

// ========================================================================
