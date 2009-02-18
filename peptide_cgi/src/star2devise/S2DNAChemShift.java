// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2009
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the calculation and output of chemical shift data
// for nucleic acids (DNA and RNA).  This class implements what's
// common to both DNA and RNA, with separate classes for what's different.
// For each set of chemical shift data, it creates a data file,
// a session file, an individual html file, and a link in the summary html
// file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DNAChemShift extends S2DChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    protected String[] atomNameList;//TEMPTEMP -- rename?

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DNAChemShift(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int[] resSeqCodes,
      String[] residueLabels, String[] atomNames, String[] atomTypes,
      double[] chemShiftVals, int[] ambiguityVals,
      int entityAssemblyID, String frameDetails)
      throws S2DException
    {
	super(name, longName, dataDir, sessionDir, summary, resSeqCodes,
	  residueLabels, atomNames, atomTypes, chemShiftVals,
	  ambiguityVals, entityAssemblyID, frameDetails);

        if (doDebugOutput(11)) {
	    System.out.println("S2DNAChemShift.S2DNAChemShift(" + name +
	      ")");
	}
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltaShifts(int frameIndex, String schemaName,
      String attributes) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNAChemShift.writeDeltashifts()");
	}

	//
	// Write the deltashift values to the appropriate data file.
	//
        FileWriter deltashiftWriter = null;
	try {
            deltashiftWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.DELTASHIFT_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    deltashiftWriter.write("# Data: delta shift values for " +
	      _name + "\n");
	    deltashiftWriter.write("# Schema: " + schemaName + "\n");
	    deltashiftWriter.write("# Attributes: " + attributes + "\n");
            deltashiftWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            deltashiftWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    deltashiftWriter.write("#\n");

        } catch(IOException ex) {
	    System.err.println("IOException writing deltashifts: " +
	      ex.toString());
	    throw new S2DError("Can't write deltashifts");
	}

	try {
	    //TEMPTEMP -- write actual values here
	    int dsCount = 0;//TEMPTEMP

	    //
	    // Write the session file
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_NA_DELTASHIFT,
	      _name, frameIndex, _info);

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Chemical Shift Delta (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_DELTASHIFT, _name, frameIndex,
	      title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeDeltashift(frameIndex, _entityAssemblyID,
	      dsCount);

	} catch (IOException ex) {
	    System.err.println("IOException writing deltashift data: " +
	      ex.toString());
	    throw new S2DError("Unable to write deltashift data for " +
	      frameIndex);
	} finally {
	    try {
	        deltashiftWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write the CSI info for this data.
    public void writeCSI(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.writeCSI()");
	}

	// Don't do anything for nucleic acids.
    }

    //-------------------------------------------------------------------
    // Write the percent assignments for this data.
    // For now, do nothing for nucleic acids.
    public void writePctAssign(int frameIndex, boolean checkPctAssign,
      String chemAssgFile)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNAChemShift.writePctAssign()");
	}

	// Don't do anything for nucleic acids.
    }

    //===================================================================
    // PROTECTED METHODS
    //-------------------------------------------------------------------
    // Calculate all delta shifts for this data set.
    protected void calculateDeltaShifts() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNAChemShift.calculateDeltaShifts()");
	}

	//TEMPTEMP -- do actual calculation here
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
