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

// This class implements the output of heteronuclear NOE data.  For each set
// of heteronuclear NOE data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2001/03/08 20:33:24  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.1.2.1  2001/02/09 16:57:12  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//

// ========================================================================

import java.io.*;

public class S2DHetNOE {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accessionNum;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private String _name;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _hetNOEValues;
    private String[] _hetNOEErrors;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DHetNOE(int accessionNum, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String frequency, String atom1Name,
      String atom2Name,
      String[] resSeqCodes, String[] resLabels,
      String[] hetNOEValues, String[] hetNOEErrors) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DHetNOE.S2DHetNOE(" + accessionNum + ")");
	}
        _accessionNum = accessionNum;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;

	_name = "Heteronuclear NOE (" + frequency + " MHz) " + atom1Name +
	  " " + atom2Name;

        _resSeqCodes = resSeqCodes;
        _resLabels = resLabels;
        _hetNOEValues = hetNOEValues;
        _hetNOEErrors = hetNOEErrors;
    }

    //-------------------------------------------------------------------
    // Write the heteronuclear NOE values for this data.
    public void writeHetNOE(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DHetNOE.writeHetNOE()");
	}

	try {
	    //
	    // Write the heteronuclear NOE values to the data file.
	    //
            FileWriter hetNOEWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.HETERONUCLEAR_NOE_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);

	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        hetNOEWriter.write(_resSeqCodes[index] + " " +
		  _resLabels[index] + " " + _hetNOEValues[index] + " " +
		  _hetNOEErrors[index] + "\n");
	    }

	    hetNOEWriter.close();

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_HETNOE,
	      _accessionNum, frameIndex, _name);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_HETNOE,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeHetNOE(_name, frameIndex, _resSeqCodes.length);

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing heteronuclear NOE values: " +
	      ex.getMessage());
	    throw new S2DError("Can't write heteronuclear NOE values");
	}
    }
}

// ========================================================================
