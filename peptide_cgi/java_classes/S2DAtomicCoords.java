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

//TEMPTEMP
// This class implements the output of heteronuclear NOE data.  For each set
// of heternoclear NOE data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ========================================================================

import java.io.*;

public class S2DAtomicCoords {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accessionNum;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _atomNames;
    private String[] _atomTypes;
    private double[] _atomCoordX;
    private double[] _atomCoordY;
    private double[] _atomCoordZ;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DAtomicCoords(int accessionNum, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String[] resSeqCodes, String[] resLabels,
      String[] atomNames, String[] atomTypes, double[] atomCoordX,
      double[] atomCoordY, double[] atomCoordZ) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DAtomicCoords.S2DAtomicCoords(" +
	      accessionNum + ")");
	}
        _accessionNum = accessionNum;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;

        _resSeqCodes = resSeqCodes;
        _resLabels = resLabels;
	_atomNames = atomNames;
	_atomTypes = atomTypes;
	_atomCoordX = atomCoordX;
	_atomCoordY = atomCoordY;
	_atomCoordZ = atomCoordZ;
    }

    //-------------------------------------------------------------------
    // Write the atomic coordinates for this data.
    public void writeAtomicCoords(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DAtomicCoords.writeAtomicCoords()");
	}

	try {
	    //
	    // Write the atomic coordinates to the data file.
	    //
            FileWriter coordWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.ATOMIC_COORD_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);

	    String lastRes = "";
	    int resCount = 0;
	    int atomCount = 0;
	    for (int index = 0; index < _resSeqCodes.length; index++) {
		if (!_resSeqCodes[index].equals(".")) {
	            coordWriter.write(_resSeqCodes[index] + " " +
		      _resLabels[index] + " " + _atomNames[index] + " " +
		      _atomTypes[index] + " " + _atomCoordX[index] + " " +
		      _atomCoordY[index] + " " + _atomCoordZ[index] + " " +
		      "\n");
		    if (!lastRes.equals(_resSeqCodes[index])) {
		        resCount++;
			lastRes = _resSeqCodes[index];
		    }
		    atomCount++;
	        }
	    }

	    coordWriter.close();

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_ATOMIC_COORDS,
	      _accessionNum, frameIndex);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_ATOMIC_COORDS,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeAtomicCoords(frameIndex, resCount, atomCount);

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing atomic coordinates: " +
	      ex.getMessage());
	    throw new S2DError("Can't write atomic coordinates");
	}
    }
}

// ========================================================================
