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
// Revision 1.1  2009/02/18 21:43:07  wenger
// Added S2DNAChemShift class to clean up nucleic acid code (this class
// will do the actual calculation and writing of chemical shift deltas
// for nucleic acids); added schemas for nucleic acid deltashift
// visualizations; updated tests to reflect the fact that (at least
// initially) we're not going to generate CSI visualizations for nucleic
// acids.
//
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DNAChemShift extends S2DChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    // List of all possible atom names for the relevant type of
    // polymer (DNA or RNA) -- set by subclass.
    protected String[] atomNameList;

    protected ResidueInfo[] _deltashiftData;

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
            deltashiftWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.DELTASHIFT_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);
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
	    int dsCount = 0;//TEMPTEMP?

	    for (int index = 1; index < _deltashiftData.length; index++) {
		if (_deltashiftData[index] != null) {
	            _deltashiftData[index].write(deltashiftWriter);
		}
	    }

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
	
	int lastResidue = findLastResidue();

	// Residues normally start with 1 -- skip the first element of
	// this array to make things simpler.
	_deltashiftData = new ResidueInfo[lastResidue + 1];

	for (int index = 0; index < _resSeqCodes.length; ++index) {
	    int resSeqCode = _resSeqCodes[index];
	    if (_deltashiftData[resSeqCode] == null) {
	        _deltashiftData[resSeqCode] = new ResidueInfo(resSeqCode);
	    }

            String resLabel = _residueLabels[index];
	    String atomName = _atomNames[index];
	    double chemShift = _chemShiftVals[index];

	    try {
	        ShiftDataManager.Pair standardValue =
		  _refTable.returnValues(resLabel, atomName);

                // Note: do the calculation in double and truncate to float
		// to avoid getting values like 0.05000000000000071.
		float deltashift = (float)(chemShift -
		  standardValue.chemshift);

		_deltashiftData[resSeqCode].put(atomName,
		  new Float(deltashift));

	    } catch(S2DWarning ex) {
	        if (doDebugOutput(11)) {
	            System.err.println(ex.toString());
	        }
	    } catch(S2DException ex) {
	        System.err.println(ex.toString());
	    }



	}
    }

    //-------------------------------------------------------------------
    // Class to hold all deltashift info for one residue -- maps
    // atom name to deltashift value.
    protected class ResidueInfo extends Hashtable
    {
	int _residueSequenceCode;

        //---------------------------------------------------------------
	/**
	 * Constructor.
	 * @param The residue sequence code for this residue.
	 */
        public ResidueInfo(int residueSequenceCode) {
	    super();

	    _residueSequenceCode = residueSequenceCode;

	    // Add all possible atoms to the hash table.
	    for (int index = 0; index < atomNameList.length; index++) {
	    	Float value = new Float(0.0/*TEMP?*/);
		put(atomNameList[index], value);
	    }
	}

        //---------------------------------------------------------------
	/**
	 * Write the deltashift values for this residue.
	 * @param The FileWriter to write to.
	 */
	public void write(FileWriter writer) throws IOException
	{
	    writer.write(_entityAssemblyID + " " +
	      _residueSequenceCode + " ");

	    for (int index = 0; index < atomNameList.length; index++) {
	    	Float value = (Float)get(atomNameList[index]);
		writer.write(value + " ");
	    }
	    writer.write("\n");
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
