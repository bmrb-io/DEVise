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

// This class implements the output of coupling constants.  For each set of
// coupling constants, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1  2001/01/17 20:00:06  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

// ========================================================================

import java.io.*;

public class S2DCoupling {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accessionNum;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private String[] _couplingConstCodes;
    private String[] _atom1ResSeqs;
    private String[] _atom1ResLabels;
    private String[] _atom1Names;
    private String[] _atom2ResSeqs;
    private String[] _atom2ResLabels;
    private String[] _atom2Names;
    private String[] _couplingConstValues;
    private String[] _couplingConstErrors;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DCoupling(int accessionNum, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String[] couplingConstCodes,
      String[] atom1ResSeqs, String[] atom1ResLabels, String[] atom1Names,
      String[] atom2ResSeqs, String[] atom2ResLabels, String[] atom2Names,
      String[] couplingConstValues, String[] couplingConstErrors)
      throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DCoupling.S2DCoupling(" + accessionNum +
	      ")");
	}
        _accessionNum = accessionNum;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;

        _couplingConstCodes = couplingConstCodes;
        _atom1ResSeqs = atom1ResSeqs;
        _atom1ResLabels = atom1ResLabels;
        _atom1Names = atom1Names;
        _atom2ResSeqs = atom2ResSeqs;
        _atom2ResLabels = atom2ResLabels;
        _atom2Names = atom2Names;
        _couplingConstValues = couplingConstValues;
        _couplingConstErrors = couplingConstErrors;
    }

    //-------------------------------------------------------------------
    // Write the coupling constansts for this data.
    public void writeCoupling(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DCoupling.writeCoupling()");
	}

	try {
	    //
	    // Write the coupling constant values to the data file.
	    //
            FileWriter couplingWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.COUPLING_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);

	    for (int index = 0; index < _couplingConstValues.length; index++) {
	        couplingWriter.write(_couplingConstCodes[index] + " " +
		  _atom1ResSeqs[index] + " " + _atom1ResLabels[index] +
		  " " + _atom1Names[index] + " " + _atom2ResSeqs[index] +
		  " " + _atom2ResLabels[index] + " " + _atom2Names[index] +
		  " " + _couplingConstValues[index] + " " +
		  //TEMP _couplingConstErrors[index] +
		  "\n");
	    }

	    couplingWriter.close();

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_COUPLING,
	      _accessionNum, frameIndex);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_COUPLING,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeCoupling(frameIndex, _couplingConstValues.length);

        } catch(IOException ex) {
	    System.err.println("IOException writing coupling constants: " +
	      ex.getMessage());
	    throw new S2DError("Can't write coupling constants");
	}
    }
}

// ========================================================================
